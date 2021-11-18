// Import and init express
const express = require('express')
const app = express();

// Import and init dotenv
const dotenv = require('dotenv');
dotenv.config();

// Import routers
const user_router = require('./router/user_router.js');
const event_router = require('./router/event_router.js');

// Init necessary variables
var port = process.env.PORT;

// Middlewares
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use('/user', user_router);
app.use('/event', event_router);

// Start application
app.listen(port, () => {
    console.log(`BankSimul REST api listening at http://localhost:${port}`)
})

module.exports = app