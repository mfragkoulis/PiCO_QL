# -*- coding: utf-8 -*-

from flask import Flask, request, session, g, redirect, url_for, \
     abort, render_template, render_template_string, flash, jsonify
from urlparse import urlparse
import re
import json
from httplib import HTTPConnection, HTTPSConnection
from urlparse import urlparse, parse_qsl, urljoin
from urllib import urlencode, unquote
from werkzeug.exceptions import HTTPException, Forbidden
from time import sleep

picoql = Flask("picoQL-web-service")

# The view for the app's intro screen.
@picoql.route('/', methods = ['GET', 'POST'])
def app_home():
    return render_template('home.html')

@picoql.route('/ask')
def query():
    response_type = request.headers['Http_Choose_Response_Type']
    #print request.headers
    query = "SELECT * FROM SuperAccounts;"
    conn = HTTPConnection("localhost:8081")
    params = urlencode({ 'query' : query })
    path = '/serveQuery.html?%s' % params
    headers = { 'Http_Choose_Response_Type' : response_type }
    conn.request('GET', path, headers=headers)
    response = conn.getresponse()
    resultset = response.read()
    print "response is: %s" % resultset
    if response.status != 200:
        logger.error("Call to retrieve query result set failed with code %d." %
                         response.status)
        raise Exception("Call to retrieve query result set failed with code %d." %
                         response.status)

    response_type = response.getheader('Content-type')
    return jsonify(resultset=resultset)

if __name__ == '__main__':
    picoql.run(debug=True)
