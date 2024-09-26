#! /usr/bin/python

import glob
import subprocess

PASS = '\033[92m'
FAIL = '\033[91m'
COLOR_OFF = '\033[0m'

test_files = glob.glob('**/*.js',recursive=True)
failed_tests = []

for test_file in test_files:
	expect_file_name = test_file + '.expect'
	with open(expect_file_name) as file:
		expected_result = file.read()
	
	print(f'{COLOR_OFF}{test_file} - ', end='')
	try:
		proc = subprocess.run(['../../../../build/js', test_file], stdout=subprocess.PIPE)
		result = proc.stdout.decode('utf-8').strip()
		
		if result == expected_result:
			print(f'{PASS}PASS')
		else:
			failed_tests.append(test_file)
			print(f'{FAIL}FAIL')
			print('expect:')
			print(f'{expected_result}\n')
			print('got:')
			print(f'{result}')
	
	except:
		print(f'{FAIL}parser error')


print(f'{COLOR_OFF}')
if len(failed_tests) == 0:
	print('All tests passed')
else:
	print('Failed Tests:')
	for failure in failed_tests:
		print(failure)
