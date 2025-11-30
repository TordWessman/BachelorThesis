#!/usr/bin/env python3
from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import sqlite3
import random
import time

DB_NAME = 'py.remote.db'

def init_db(db_path: str):
    conn = sqlite3.connect(db_path)
    cur = conn.cursor()
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS tx (
            chainId INTEGER,
            batchId INTEGER,
            costG INTEGER,
            sendTime INTEGER,
            transactionDuration INTEGER,
            confirmationDuration INTEGER,
            transactionHash TEXT,
            gasUsed TEXT
        )
        """
    )
    cur.execute(
        """
        CREATE TABLE IF NOT EXISTS tx_error (
            errorStep INTEGER,
            chainId INTEGER,
            batchId INTEGER,
            description TEXT,
            code INTEGER,
            sendTime INTEGER,
            transactionHash TEXT
        )
        """
    )
    conn.commit()
    return conn

class Handler(BaseHTTPRequestHandler):
    db_conn: sqlite3.Connection = None
    batch_id: int = None

    def _read_json(self):
        length = int(self.headers.get('Content-Length', 0))
        data = self.rfile.read(length) if length > 0 else b''
        try:
            return json.loads(data.decode('utf-8') or '{}')
        except json.JSONDecodeError:
            return {"_raw": data.decode('utf-8', errors='replace')}

    def do_POST(self):
        payload = self._read_json()
        if self.path == '/success':
            print('[SUCCESS]', json.dumps(payload, ensure_ascii=False))
            self._store_success(payload)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b'ok')
        elif self.path == '/failure':
            print('[FAILURE]', json.dumps(payload, ensure_ascii=False))
            self._store_failure(payload)
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b'ok')
        else:
            print("[UNKNOWN PATH]", self.path)
            self.send_response(404)
            self.end_headers()
            self.wfile.write(b'not found')

    def _store_success(self, payload: dict):
        chainId = int(payload.get('chainId', 0))
        sendTime = int(payload.get('sendTime', int(time.time())))
        transactionDuration = int(payload.get('transactionDuration', 0))
        confirmationDuration = int(payload.get('confirmationDuration', 0))
        transactionHash = str(payload.get('transactionHash') or '')
        gasUsed = str(payload.get('gasUsed') or '')
        costG = int(payload.get('costG', 0))
        self.db_conn.execute(
            "INSERT INTO tx (chainId, batchId, costG, sendTime, transactionDuration, confirmationDuration, transactionHash, gasUsed) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
            (chainId, Handler.batch_id, costG, sendTime, transactionDuration, confirmationDuration, transactionHash, gasUsed)
        )
        self.db_conn.commit()

    def _store_failure(self, payload: dict):
        errorStep = int(payload.get('errorStep', 0))
        chainId = int(payload.get('chainId', 0))
        sendTime = int(payload.get('sendTime', int(time.time())))
        description = str(payload.get('description') or '')
        code = int(payload.get('code', 0))
        transactionHash = str(payload.get('transactionHash') or '')
        self.db_conn.execute(
            "INSERT INTO tx_error (errorStep, chainId, batchId, description, code, sendTime, transactionHash) VALUES (?, ?, ?, ?, ?, ?, ?)",
            (errorStep, chainId, Handler.batch_id, description, code, sendTime, transactionHash)
        )
        self.db_conn.commit()


def run(host='0.0.0.0', port=9999):
    conn = init_db(DB_NAME)
    Handler.db_conn = conn
    Handler.batch_id = random.randint(1, 2**31 - 1)
    server = HTTPServer((host, port), Handler)
    print(f'Serving on {host}:{port} (POST /success, /failure)')
    print(f'Database: {DB_NAME}, batchId: {Handler.batch_id}')
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print('Shutting down...')
        server.server_close()

if __name__ == '__main__':
    run()
