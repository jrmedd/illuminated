from flask import Flask, flash, url_for, render_template, request, redirect, make_response, Response, jsonify
from flask_socketio import SocketIO
from bson import ObjectId

import datetime

from pymongo import MongoClient

import datetime

import os

MONGO_URL = os.environ.get('MONGO_URL')

CLIENT = MongoClient(MONGO_URL)
DB = CLIENT['illuminated']
RHYTHMS = DB['rhythms']

APP = Flask(__name__)

APP.secret_key = os.environ.get('SECRET_KEY')

SOCKETIO = SocketIO(APP)

@APP.route('/', methods=['GET'])
def index():
  return render_template('index.html')

@APP.route('/rhythms', methods=['GET'])
def rhythms():
    to_illuminate = RHYTHMS.find_one({'illuminated':False},sort=[("timestamp", 1)])
    to_illuminate.update({'_id':str(to_illuminate.get('_id'))}) #after this, mark as illuminated!
    return jsonify(to_illuminate=to_illuminate)

@SOCKETIO.on('illuminate')
def handle_message(message):
    rhythm_to_store = message
    rhythm_to_store.update({'illuminated':False, 'timestamp':datetime.datetime.now()})
    RHYTHMS.insert(rhythm_to_store)
    return jsonify(stored=True)



if __name__ == '__main__':
    SOCKETIO.run(APP, host="0.0.0.0", debug=True)
