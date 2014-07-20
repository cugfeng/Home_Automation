#!/usr/bin/python

import cgi, cgitb
import os

TEMP_SETTING_DIR       = "/tmp/temperature"
TEMP_SETTING_CURRENT   = "/tmp/temperature/current"
TEMP_SETTING_TARGET    = "/tmp/temperature/target"
TEMP_SETTING_TOLERANCE = "/tmp/temperature/tolerance"
TEMP_SETTING_EXIT      = "/tmp/temperature/exit"

if os.access(TEMP_SETTING_DIR, os.F_OK) is False:
	os.mkdir(TEMP_SETTING_DIR, 0777)

form      = cgi.FieldStorage()
target    = form.getvalue('target')
tolerance = form.getvalue('tolerance')

fd_target = open(TEMP_SETTING_TARGET, "w+")
fd_target.write(str(int(target) * 1000))
fd_target.close()

fd_tolerance = open(TEMP_SETTING_TOLERANCE, "w+")
fd_tolerance.write(str(int(tolerance) * 1000))
fd_tolerance.close()

print "Content-type:text/html\n\n"
print "<html>"
print "<head>"
print "<title>CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Target temperature   : %s</h2>" % (target)
print "<h2>Tolerance temperature: %s</h2>" % (tolerance)
print "</body>"
print "</html>"
