#include "Label.h"

mdns::Label::Label(String name, Label * nextLabel, bool caseSensitive) {
  data = (uint8_t *) malloc(name.length() + 1);

  if (data) {
    data[0] = name.length();
    for (uint8_t i = 0; i < name.length(); i++) {
      data[i + 1] = name.charAt(i);
    }
  } else {
    data = EMPTY_DATA;
  }

  this->nextLabel = nextLabel;
  this->caseSensitive = caseSensitive;
}

uint8_t mdns::Label::getSize() {
  return data[0];
}

uint8_t mdns::Label::getWriteSize() {
  Label * label = this;
  uint8_t size = 0;

  while (label != NULL) {
    if (label->writeOffset == INVALID_OFFSET) {
      size += label->data[0] + 1;
      label = label->nextLabel;
    } else {
      size += 2;
      label = NULL;
    }
  }

  return size;
}

void mdns::Label::write(Buffer * buffer) {
  Label * label = this;

  while (label) {
    if (label->writeOffset == INVALID_OFFSET) {
      label->writeOffset = buffer->getOffset();

      uint8_t size = label->data[0] + 1;

      for (uint8_t i = 0; i < size; i++) {
        buffer->writeUInt8(label->data[i]);
      }

      label = label->nextLabel;
    } else {
      buffer->writeUInt16((LABEL_POINTER << 8) | label->writeOffset);
      label = NULL;
    }
  }
}

void mdns::Label::reset() {
  Label * label = this;

  while (label != NULL) {
    label->writeOffset = INVALID_OFFSET;

    label = label->nextLabel;
  }
}

mdns::Label::Reader::Reader(Buffer * buffer) {
  this->buffer = buffer;
}

bool mdns::Label::Reader::hasNext() {
  return c != END_OF_NAME && buffer->available() > 0;
}

uint8_t mdns::Label::Reader::next() {
  //Serial.println(" mdns::Label::Reader::next: [1]");
  c = buffer->readUInt8();
  //Serial.println(" mdns::Label::Reader::next: [2]");
  while ((c & LABEL_POINTER) == LABEL_POINTER) {
    //Serial.println(" mdns::Label::Reader::next: [3]");

    if (buffer->available() > 0) {
      //Serial.println(" mdns::Label::Reader::next: [4]");
      uint8_t c2 = buffer->readUInt8();
      //Serial.println(" mdns::Label::Reader::next: [5]");
      uint16_t pointerOffset = ((c & ~LABEL_POINTER) << 8) | c2;
      //Serial.println(" mdns::Label::Reader::next: [6]");
      buffer->mark();
      //Serial.println(" mdns::Label::Reader::next: [7]");
      buffer->setOffset(pointerOffset);
      //Serial.println(" mdns::Label::Reader::next: [8]");
      c = buffer->readUInt8();
      //Serial.println(" mdns::Label::Reader::next: [9]");
    }
    else
    {
      Serial.println("mdns::Label::Reader::next():: AVOIDING INFINITE LOOP, RETURNING!");
      return c;
    }

    //Serial.println(" mdns::Label::Reader::next: [10]");
  }
  //Serial.println(" mdns::Label::Reader::next: [11]");
  return c;
}

bool mdns::Label::Reader::endOfName() {
  return c == END_OF_NAME;
}

mdns::Label::Iterator::Iterator(Label * label) {
  this->label = label;
  this->startLabel = label;
  this->size = label->data[0];
}

bool mdns::Label::Iterator::match(uint8_t c) {
  if (matches) {
    while (offset > size && label) {
      label = label->nextLabel;
      size = label->data[0];
      offset = 0;
    }

    matches = offset <= size && label && (label->data[offset] == c || (!label->caseSensitive && equalsIgnoreCase(c)));

    offset++;
  }

  return matches;
}

bool mdns::Label::Iterator::matched() {
  return matches;
}

bool mdns::Label::Iterator::equalsIgnoreCase(uint8_t c) {
  return (c >= 'a' && c <= 'z' && label->data[offset] == c - 32) || (c >= 'A' && c <= 'Z' && label->data[offset] == c + 32);
}

mdns::Label * mdns::Label::Iterator::getStartLabel() {
  return startLabel;
}

mdns::Label * mdns::Label::Matcher::match(std::map<String, Label *> labels, Buffer * buffer) {

  //Serial.println(" mdns::Label::Matcher::match: [1]");
  Iterator * iterators[labels.size()];
  //Serial.println(" mdns::Label::Matcher::match: [2]");
  std::map<String, Label *>::const_iterator i;

  uint8_t idx = 0;
  //Serial.println(" mdns::Label::Matcher::match: [3]");
  for (i = labels.begin(); i != labels.end(); ++i) {
    iterators[idx++] = new Iterator(i->second);
  }
    //Serial.println(" mdns::Label::Matcher::match: [4]");

  Reader * reader = new Reader(buffer);
  //Serial.println(" mdns::Label::Matcher::match: [5]");
  while (reader->hasNext()) {
   //   Serial.println(" mdns::Label::Matcher::match: [6]");
    uint8_t size = reader->next();

    uint8_t idx = 0;
   //   Serial.println(" mdns::Label::Matcher::match: [7]");
    for (uint8_t i = 0; i < labels.size(); i++) {
      iterators[i]->match(size);
    }
   // Serial.println(" mdns::Label::Matcher::match: [8]");
    while(idx < size && reader->hasNext()) {
     // Serial.println(" mdns::Label::Matcher::match: [8.1]");
      uint8_t c = reader->next();
   //   Serial.println(" mdns::Label::Matcher::match: [8.2]");
      for (uint8_t i = 0; i < labels.size(); i++) {
  //      Serial.println(" mdns::Label::Matcher::match: [8.3]");
        iterators[i]->match(c);
      }
  //    Serial.println(" mdns::Label::Matcher::match: [8.4]");
      idx++;
    }
  //  Serial.println(" mdns::Label::Matcher::match: [9]");
  }

 // Serial.println(" mdns::Label::Matcher::match: [10]");
  buffer->reset();
 // Serial.println(" mdns::Label::Matcher::match: [11]");
  Label * label = NULL;
 // Serial.println(" mdns::Label::Matcher::match: [12]");
  if (reader->endOfName()) {
    uint8_t idx = 0;
 //   Serial.println(" mdns::Label::Matcher::match: [13]");
    while (label == NULL && idx < labels.size()) {
  //    Serial.println(" mdns::Label::Matcher::match: [14]");
      if (iterators[idx]->matched()) {
        label = iterators[idx]->getStartLabel();
      }
   //   Serial.println(" mdns::Label::Matcher::match: [15]");
      idx++;
    }
  }
//  Serial.println(" mdns::Label::Matcher::match: [16]");
  for (uint8_t i = 0; i < labels.size(); i++) {
    delete iterators[i];
  }
//  Serial.println(" mdns::Label::Matcher::match: [17]");
  delete reader;
 // Serial.println(" mdns::Label::Matcher::match: [18]");
  return label;
}

void mdns::Label::matched(uint16_t type, uint16_t cls) {
}

mdns::HostLabel::HostLabel(Record * aRecord, Record * nsecRecord, String name, Label * nextLabel, bool caseSensitive):Label(name, nextLabel, caseSensitive) {
  this->aRecord = aRecord;
  this->nsecRecord = nsecRecord;
}

void mdns::HostLabel::matched(uint16_t type, uint16_t cls) {
  switch(type) {
    case A_TYPE:
    case ANY_TYPE:
    aRecord->setAnswerRecord();
    nsecRecord->setAdditionalRecord();
    break;

    default:
    nsecRecord->setAnswerRecord();
  }
}

mdns::ServiceLabel::ServiceLabel(Record * aRecord, String name, Label * nextLabel, bool caseSensitive):Label(name, nextLabel, caseSensitive) {
  this->aRecord = aRecord;
}

void mdns::ServiceLabel::addInstance(Record * ptrRecord, Record * srvRecord, Record * txtRecord) {
    ptrRecords.push_back(ptrRecord);
    srvRecords.push_back(srvRecord);
    txtRecords.push_back(txtRecord);
}

void mdns::ServiceLabel::matched(uint16_t type, uint16_t cls) {
  switch(type) {
    case PTR_TYPE:
    case ANY_TYPE:
    for (std::vector<Record *>::const_iterator i = ptrRecords.begin(); i != ptrRecords.end(); ++i) {
      (*i)->setAnswerRecord();
    }
    for (std::vector<Record *>::const_iterator i = srvRecords.begin(); i != srvRecords.end(); ++i) {
      (*i)->setAdditionalRecord();
    }
    for (std::vector<Record *>::const_iterator i = txtRecords.begin(); i != txtRecords.end(); ++i) {
      (*i)->setAdditionalRecord();
    }
    aRecord->setAdditionalRecord();
    break;
  }
}

mdns::InstanceLabel::InstanceLabel(Record * srvRecord, Record * txtRecord, Record * nsecRecord, Record * aRecord, String name, Label * nextLabel, bool caseSensitive):Label(name, nextLabel, caseSensitive) {
  this->srvRecord = srvRecord;
  this->txtRecord = txtRecord;
  this->nsecRecord = nsecRecord;
  this->aRecord = aRecord;
}

void mdns::InstanceLabel::matched(uint16_t type, uint16_t cls) {
  switch(type) {
    case SRV_TYPE:
    srvRecord->setAnswerRecord();
    txtRecord->setAdditionalRecord();
    nsecRecord->setAdditionalRecord();
    aRecord->setAdditionalRecord();
    break;

    case TXT_TYPE:
    txtRecord->setAnswerRecord();
    srvRecord->setAdditionalRecord();
    nsecRecord->setAdditionalRecord();
    aRecord->setAdditionalRecord();
    break;

    case ANY_TYPE:
    srvRecord->setAnswerRecord();
    txtRecord->setAnswerRecord();
    nsecRecord->setAdditionalRecord();
    aRecord->setAdditionalRecord();
    break;

    default:
    nsecRecord->setAnswerRecord();
  }
}

mdns::MetaLabel::MetaLabel(String name, Label * nextLabel):Label(name, nextLabel) {
  // Do nothing
}

void mdns::MetaLabel::addService(Record * ptrRecord) {
  records.push_back(ptrRecord);
}

void mdns::MetaLabel::matched(uint16_t type, uint16_t cls) {
  switch(type) {
    case PTR_TYPE:
    case ANY_TYPE:
      for (std::vector<Record *>::const_iterator i = this->records.begin(); i != this->records.end(); ++i) {
        (*i)->setAnswerRecord();
      }
      break;
  }
}

