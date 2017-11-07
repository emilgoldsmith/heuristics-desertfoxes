const http = require('http');
const fs = require('fs');
const role = process.argv[2];
const { id, pid, code, hostname, port } = require(`./${role}.json`);

let inputURL = `http://${hostname}:${port}/api/get?role=${role}&id=${id}&pid=${pid}&code=${code}`;

http.get(inputURL, res => {
  res.setEncoding('utf8');
  let body = '';
  res.on('data', data => {
    body += data;
  });
  res.on('end', () => {
    body = JSON.parse(body);
    console.log(body);
    fs.writeFileSync(`${role}.in`, body.data);
  });
});
