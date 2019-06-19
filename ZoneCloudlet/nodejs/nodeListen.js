#!/usr/bin/env node
var io = require('socket.io').listen(9000);
var io2 = require('socket.io').listen(9001);
var pg = require ('pg');

//var con_string = 'tcp://osm:vehicle2016@localhost/testdb';
//var con_string = "postgres://osm:vehicle2016@localhost/testdb";
var con_string = 'postgresql://osm:vehicle2016@localhost:5432/testdb';
var pg_client = new pg.Client(con_string);
pg_client.connect();
//var pg_client2 = new pg.Client(con_string);
//pg_client2.connect();
const Query = pg.Query;

//var query2 = pg_client.query('LISTEN "addrecord"');

io.sockets.on('connection', function (socket) {
    socket.emit('Connected', { connected: true });
    console.log('Connected');
    socket.on('Ready for Data', function (data) {
        var query2 = pg_client.query(new Query("SELECT row_to_json(hazards) FROM hazards"))
        query2.on('row', (row) => {
            console.log(row);
            //var obj = JSON.parse(row_to_json);
            console.log(row.row_to_json.image);
            socket.emit('Initial', row );
        })
        query2.on('end', (res) => {
            console.log('Complete');
            socket.emit('Complete', {});
        })
    });
    socket.on('Ready for More Data', function (data) {
        var query = pg_client.query('LISTEN "addrecord"');
        pg_client.on('notification', function(msg) {
            console.log(msg);
            socket.emit('Update', { message: msg });
        });
    });
});

io2.sockets.on('connection', function (socket2) {
    socket2.emit('Connected', { connected: true });
    console.log('Connected');
    socket2.on('Ready for Data', function (data) {
        var query2 = pg_client.query(new Query("SELECT row_to_json(drive) FROM drive"))
        query2.on('row', (row) => {
            console.log(row);
            //var obj = JSON.parse(row_to_json);
            //console.log(row.row_to_json.image);
            socket2.emit('Initial', row );
        })
        query2.on('end', (res) => {
            console.log('Complete');
            socket2.emit('Complete', {});
        })
    });
    socket2.on('Ready for More Data', function (data) {
        var query3 = pg_client.query('LISTEN "addDrive"');
        pg_client.on('notification', function(msg) {
            console.log(msg);
            socket2.emit('Update', { message: msg });
        });
    });
});