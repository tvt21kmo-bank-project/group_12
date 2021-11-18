const express = require('express')

const app = express();
const dotenv = require('dotenv');
dotenv.config();

var port = process.env.PORT;

app.listen(port, () => {
    console.log(`BankSimul REST api listening at http://localhost:${port}`)
})