const http = require('http');
const { numPackages, numVersions, numCompatibles, hostname, port, date1, date2, date3 } = require('./localtest.json');

// create a new contest
const createURL = `http://${hostname}:${port}/api/create?date1=${date1}&date2=${date2}&date3=${date3}&numpackages=${numPackages}&numversions=${numVersions}&numcompatibles=${numCompatibles}`
http.get(createURL);
