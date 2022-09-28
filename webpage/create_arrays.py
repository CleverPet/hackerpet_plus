


import sys
import os
import subprocess


def main():

    N_arrays = 40
    # TODO THIS IS JUST A SIZE EXPERIMENT!
    # THIS WON'T WORK ON PARTICLE
    # IT REFERENCES .css, .js that are not included!
    # testing whether size is good, how much remains, if those are assumed loaded from elsewhere
    # we will need text version backup in this case

    #input_file = 'cleverpet_minified/index.html'

    input_file = 'index_combined.html'

    # use work folder

    whole_string = ''
    #lines = []
    f = open(input_file, 'r')

    all_cpp_lines = ''

    done = False
    while not done:
        line = f.readline()

        # strip out whitespace at start of line
        line = line.lstrip(' ')

        #print(int(line[-1]))
        if line:
            #lines.append(line)
            whole_string += line
        else:
            done = True

    print(len(whole_string))
    
    chars_per_array = int(len(whole_string) / N_arrays)

    for k in range(N_arrays):
        # print this section to a tmp html file
        tmp_html_file = 'work/html_piece_' + str(k) + '.tmp'
        tmp_out_file = 'work/cpp_piece_' + str(k) + '.h'
        f_out = open(tmp_html_file, 'w')

        l_start = k * chars_per_array
        l_end = (k + 1) * chars_per_array
        
        if k == N_arrays - 1:
            l_end = len(whole_string)

        print(l_start, l_end, l_end-l_start)

        lines_to_write = whole_string[l_start:l_end]
        f_out.write(lines_to_write)
        f_out.close()

        # run bin2c
        subprocess.run(["bin2c", "-H", tmp_out_file, tmp_html_file])

        # load line and append to all lines
        f2 = open(tmp_out_file, 'r')
        code_line = None
        for line in f2.readlines():
            if line.startswith('static const unsigned char'):
                code_line = line
                code_line = code_line.replace('static const unsigned char', 'static const char')

        if code_line is not None:
            all_cpp_lines += code_line
        else:
            print("ERROESJPOGJSIOPDGOSDHGIOHSDIGHSDUIGIZXSDJBGHSDUIHGJUISDHGFUIOSDBH!@!!!!!!")
            assert False

    f.close()

    f = open('complete_html_code.h', 'w')
    f.write(all_cpp_lines)
    f.close()


    for k in range(N_arrays):
        #print('Log.info(bin2c_html_piece_' + str(k) + '_tmp);')
        print('_webclient.print(bin2c_html_piece_' + str(k) + '_tmp);')


if __name__ == '__main__':
    main()