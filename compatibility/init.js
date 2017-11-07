const http = require('http');
const fs = require('fs');
const { numPackages, numVersions, numCompatibles, hostname, port, date1, date2, date3, poserName, solverName } = require('./init.json');

// create a new contest
const createURL = `http://${hostname}:${port}/api/create?date1=${date1}&date2=${date2}&date3=${date3}&numpackages=${numPackages}&numversions=${numVersions}&numcompatibles=${numCompatibles}`
http.get(createURL, res => {
  res.setEncoding('utf8');
  let body = '';
  res.on('data', data => {
    body += data;
  });
  res.on('end', () => {
    body = JSON.parse(body);
    register(body.id, 'poser');
    register(body.id, 'solver');
  })
});

// register for a contest
let register = (contestID, role) => {
  let name = role === 'poser' ? poserName : solverName;
  let filename = role === 'poser' ? 'poser.json' : 'solver.json';
  let registerURL = `http://${hostname}:${port}/api/register?name=${name}&id=${contestID}`;
  http.get(registerURL, res => {
    res.setEncoding('utf8');
    let body = '';
    res.on('data', data => {
      body += data;
    });
    res.on('end', () => {
      body = JSON.parse(body);
      let filedata = {
        'id': contestID,
        'pid': poserName, // always compete on poser's problem
        'code': body.code,
        'hostname': hostname,
        'port': port
      };
      fs.writeFileSync(filename, JSON.stringify(filedata));
    });
  });
};
