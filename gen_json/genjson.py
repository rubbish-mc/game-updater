# -*- coding: utf-8 -*-
# genjson.py
# Copyright (C) 2018 Too-Naive
#
# This module is part of libpy and is released under
# the AGPL v3 License: https://www.gnu.org/licenses/agpl-3.0.txt
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
from __future__ import print_function
import json
import sys
import os
import datetime
import re
import hashlib

yes_re = re.compile(r'^[yY]$')
no_re = re.compile(r'^[nN]$')
diff_re = re.compile(r'^[+-] .+$')
filename_re = re.compile(r'^.+$')

def gen_current_time():
	return datetime.datetime.now().strftime('%Y%m%d%H%M%S')

def sha256_checksum(filename, block_size=65536):
	sha256 = hashlib.sha256()
	with open(filename, 'rb') as f:
		for block in iter(lambda: f.read(block_size), b''):
			sha256.update(block)
	return sha256.hexdigest()

def read_with_re(prompt, r, error_msg='Input error, please try again: '):
	string = raw_input(prompt)
	while r.match(string) is None:
		string = raw_input(error_msg)
	return string

def main():
	
	try:
		with open('LATEST') as fin:
			old_j = json.load(fin)
	except IOError:
		old_j = {}
		old_j['version'] = {'major': 1, 'minor': -1}

	new_j = {}
	new_j['version'] = {'major': old_j['version']['major'], 'minor': old_j['version']['minor']}

	print('Current version: {}.{}'.format(*(y for x,y in new_j['version'].items())))
	if yes_re.match(raw_input('Do you want to change major version? [y/N]: ')):
		new_j['version']['major'] += 1
		new_j['version']['minor'] = 0
	else:
		new_j['version']['minor'] += 1

	new_j['info'] = {
		'diff':
			{'filename': read_with_re('Please input `diff\' filename: ', filename_re)},
		'full':
			{'filename': read_with_re('Please input `full\' filename: ', filename_re)},
		'complete':
			{'filename': read_with_re('Please input `complete\' filename: ', filename_re)}
		}

	for _ ,_info in new_j['info'].items():
		new_j['info'][_]['filename'] += '.zip'

	new_j['info']['diff'].update({'size': os.stat(new_j['info']['diff']['filename']).st_size, 
		'checksum': sha256_checksum(new_j['info']['diff']['filename'])})
	new_j['info']['full'].update({'size': os.stat(new_j['info']['full']['filename']).st_size, 
		'checksum': sha256_checksum(new_j['info']['full']['filename'])})
	new_j['info']['complete'].update({'size': os.stat(new_j['info']['complete']['filename']).st_size, 
		'checksum': sha256_checksum(new_j['info']['complete']['filename'])})

	print('Please enter diff info, Press EOF to exit:\n  Hint:\n    `+ modname\' for new mod\n    `- modname` for delete mod')

	new_j['diff'] = []
	try:
		while True:
			new_j['diff'].append(read_with_re('', diff_re, '\ndiff message format error, try again:\n'))
	except EOFError:
		pass

	new_j['timestamp'] = gen_current_time()

	print('Json preview:')
	print(json.dumps(new_j, sort_keys=True, indent=4, separators=(',', ': ')))
	if no_re.match(raw_input('Write json file to disk? [Y/n]: ')):
		return
	try:
		os.rename('LATEST', 'mc_json_{}'.format(old_j['timestamp']))
	except Exception:
		pass
	try:
		with open('LATEST', 'w') as fout:
			fout.write(json.dumps(new_j, sort_keys=True, indent=4, separators=(',', ': ')))
	except IOError:
		print('Cannot write to `LATEST\' file, please try again')

def init():
	reload(sys)
	sys.setdefaultencoding('utf8')

if __name__ == '__main__':
	init()
	main()

