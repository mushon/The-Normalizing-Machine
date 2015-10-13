# this script is used to bootstrap TNM. It takes the first 4 recording and
# creates a valid dataset.json file. there should be no other files in the
# directory apart from 4 recordings (more recordings will be ignored)
# WARNING: this script overrides dataset.json, use carefully. anyways, it
# makes a backup in 'dataset.json.bak', if something went wrong...

import os
from time import gmtime, strftime

exclude_files = ['dataset.json',      # dataset
                 'dataset.json.bak',  # backup
                 'test1.oni',         # initial file
                 'replace4.py',       # this file
                 'dataset_template.json']  # template

file_list = []
for dirname, dirnames, filenames in os.walk('.'):

    # print path to all filenames.
    for filename in filenames:
        if filename not in exclude_files:
            file_list.append(filename)


if len(file_list) >= 4:
    print file_list


f = open('dataset_template.json', 'r')
s = f.read()
f.close()

for i in range(0, 4):  # 4 times
    print file_list[i]
    s = s.replace("place_holder" + str(i) + '.oni', file_list[i])


out_file = open('dataset.json', 'r')
bkp = out_file.read()
out_file.close()

bkp_file = open('dataset.json.bak', 'a')

bkp_file.write(
    "// this is an Archival Backup of dataset.json - DONT COPY IT AS IS\n" +
    "// every run of 'replace4.py' will append a new copy dataset.json \n" +
    "// at the end to avoid nasty accidents... "
    "// scroll down until you find what you need\n//\n" +
    "// Backup time: " +
    strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime()) + "\n")

bkp_file.write(bkp)
bkp_file.close()

out_file = open('dataset.json', 'w')
out_file.write(s)
out_file.close()
