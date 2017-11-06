const http = require('http');
const { numPackages, numVersions, numCompatibles, hostname, port, date1, date2, date3, poserName, solverName } = require('./localtest.json');

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
    register(body.id, poserName);
    register(body.id, solverName);
  })
});

// register for a contest
let register = (contestID, name) => {
  let registerURL = `http://${hostname}:${port}/api/register?name=${name}&id=${contestID}`;
  http.get(registerURL, res => {
    res.setEncoding('utf8');
    let body = '';
    res.on('data', data => {
      body += data;
    });
    res.on('end', () => {
      body = JSON.parse(body);
      console.log(body);
    })
  });
};
