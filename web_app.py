from flask import Flask, flash, url_for, render_template, request, redirect, make_response, Response, jsonify
from flask_socketio import SocketIO
from bson import ObjectId

import json

from twilio.twiml.messaging_response import Body, Message, Redirect, MessagingResponse

import datetime

from pymongo import MongoClient

import datetime

import os

MONGO_URL = os.environ.get('MONGO_URL')

CLIENT = MongoClient(MONGO_URL)
DB = CLIENT['illuminated']
RHYTHMS = DB['rhythms']

ALLOWED_KEY = os.environ.get('API_KEY')

APP = Flask(__name__)

APP.secret_key = os.environ.get('SECRET_KEY')

SOCKETIO = SocketIO(APP)

@APP.route('/', methods=['GET'])
def index():
  return render_template('index.html')

@APP.route('/rhythms', methods=['GET', 'POST'])
def rhythms():
    api_key = request.headers.get('X-Api-Key')
    if api_key != ALLOWED_KEY:
        return jsonify(valid_key=False)
    if request.method == "GET":
        to_illuminate = RHYTHMS.find_one({'illuminated':False},sort=[("timestamp", 1)])
        if to_illuminate:
            this_id = to_illuminate.get('_id')
            this_id_str = str(this_id)
            to_illuminate.update({'_id':this_id_str})
        else:
            to_illuminate = None
        return jsonify(to_illuminate=to_illuminate)
    if request.method == "POST":
        this_id = ObjectId(request.form.get('_id'))
        illuminated_status = request.form.get('illuminated')
        RHYTHMS.update({'_id':this_id}, {'$set':{'illuminated':illuminated_status}})
        return jsonify(illuminated_status=illuminated_status)


@SOCKETIO.on('connect')
def connect():
    print("Connected")

@APP.route('/illuminate', methods=['POST'])
def illuminate():
    rhythm_to_store = request.get_json()
    rhythm_to_store.update({'illuminated':False, 'timestamp':datetime.datetime.now()})
    RHYTHMS.insert(rhythm_to_store)
    return jsonify(stored=True)

@APP.route('/sms', methods=['GET', 'POST'])
def sms():
    if request.method == "POST":
        resp = MessagingResponse()
        resp.message("You sent %s" % (request.values.get('Body')))
        return jsonify(received=True)

if __name__ == '__main__':
    SOCKETIO.run(APP, host="0.0.0.0", debug=True)
