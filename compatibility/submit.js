const http = require('http');
const fs = require('fs');
const querystring = require('querystring');
const role = process.argv[2];
const { id, pid, code, hostname, port } = require(`./${role}.json`);

let inputURL = `http://${hostname}:${port}/api/get?role=${role}&id=${id}&pid=${pid}&code=${code}`;

const output = fs.readFileSync(`${role}.sol`, 'utf8');
const data = querystring.stringify({data : output});
const options = {
  hostname: hostname,
  port: port,
  path: `/api/submit?role=${role}&id=${id}&pid=${pid}&code=${code}`,
  method: 'POST',
  headers: {
    'Content-Type': 'application/x-www-form-urlencoded',
    'Content-Length': Buffer.byteLength(data)
  }
};

const req = http.request(options, res => {
  res.setEncoding('utf8');
  res.on('data', function (chunk) {
      console.log('Response: ' + chunk);
  });
});
req.write(data);
req.end();
