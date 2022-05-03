import logging

from argparse import ArgumentParser
import os
import csv

logging.basicConfig(format=u'[LINE:%(lineno)d]# %(levelname)-8s [%(asctime)s]  %(message)s',
                    level=logging.INFO)
logger = logging.getLogger(__name__)


def get_parser():
    """ create arguments parser """
    arg_parser = ArgumentParser(
        __name__, prefix_chars='--', description='Generates barcode')

    arg_parser.add_argument('-f', "--file",
                            nargs='+',
                            default='eng_rus.csv',
                            help='enter you file name')

    return arg_parser


def main():
    """ Main """
    args = get_parser().parse_args()

    logger.info(args)

    guard_start = '#ifndef WORDS_H\n'
    guard_start += '#define WORDS_H\n\n'
    guard_end = '#endif // WORDS_H\n'

    words = 'const char* words[] = {'

    count = 0
    with open(args.file, 'r', newline='', encoding="utf-8") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
        for row in spamreader:
            for col in row:
                words += '"' + col + '", '
            count += 1
    words += '};\n\n'

    define = '#define WORD_COUNT ' + str(count) + '\n\n'

    with open('words.h', 'w', encoding="utf-8") as file:
        file.write(guard_start + define + words + guard_end)

    os.system("build.py")


if __name__ == "__main__":
    main()
