#! /usr/bin/python

import glob
import subprocess

PASS = '\033[92m'
FAIL = '\033[91m'
COLOR_OFF = '\033[0m'

test_files = glob.glob('**/*.js',recursive=True)

for test_file in test_files:
	expect_file_name = test_file + '.expect'
	with open(expect_file_name) as file:
		expected_result = file.read()
	
	print(f'{COLOR_OFF}{test_file} - ', end='')
	try:
		proc = subprocess.run(['../../build/js/js', test_file], stdout=subprocess.PIPE)
		result = proc.stdout.decode('utf-8').strip()
		
		if result == expected_result:
			print(f'{PASS}PASS')
		else:
			print(f'{FAIL}FAIL')
			print('expect:')
			print(f'{expected_result}\n')
			print('got:')
			print(f'{result}')
	
	except:
		print(f'{FAIL}parser error')
