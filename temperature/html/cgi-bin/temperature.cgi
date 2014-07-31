#!/usr/bin/python

import cgi, cgitb
import os

TEMP_SETTING_DIR       = "/tmp/temperature"
TEMP_SETTING_CURRENT   = "/tmp/temperature/current"
TEMP_SETTING_TARGET    = "/tmp/temperature/target"
TEMP_SETTING_TOLERANCE = "/tmp/temperature/tolerance"
TEMP_SETTING_AUTOMODE  = "/tmp/temperature/automode"
TEMP_SETTING_EXIT      = "/tmp/temperature/exit"

if os.access(TEMP_SETTING_DIR, os.F_OK) is False:
	os.mkdir(TEMP_SETTING_DIR, 0755)
	os.chmod(TEMP_SETTING_DIR, 0777)

form = cgi.FieldStorage()

if form.getvalue('target') is None:
	target = -1
else:
	target = int(form.getvalue('target'))

if form.getvalue('tolerance') is None:
	tolerance = -1
else:
	tolerance = int(form.getvalue('tolerance'))

if form.getvalue('automode') is None:
	automode = 0
else:
	automode = 1

fd_target = open(TEMP_SETTING_TARGET, "w+")
fd_target.write(str(target * 1000))
fd_target.close()

fd_tolerance = open(TEMP_SETTING_TOLERANCE, "w+")
fd_tolerance.write(str(tolerance * 1000))
fd_tolerance.close()

fd_automode = open(TEMP_SETTING_AUTOMODE, "w+")
fd_automode.write(str(automode))
fd_automode.close()

print "Content-type:text/html\n\n"
print "<html>"
print "<head>"
print "<title>CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Target temperature   : %s</h2>" % (target)
print "<h2>Tolerance temperature: %s</h2>" % (tolerance)
print "<h2>Auto mode            : %s</h2>" % (automode)
print "</body>"
print "</html>"
