const express = require('express');
const router = express.Router();

// Login user
router.post('/login', async function(req, res){
    console.log('POST, /login');
    const cardNumber = req.body.cardNumber;
    const cardPin = req.body.cardPin;
});

// Logout user
router.get('/logout', async function(req, res){
    console.log('GET /login');
    // No params required, get necessary info from jwt
});

module.exports = router;