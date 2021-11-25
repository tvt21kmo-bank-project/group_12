const express = require('express');
const router = express.Router();
const User = require('../models/user.js')

// Login user
router.post('/login', async function(req, res){
    console.log('POST, /login');
    const cardNumber = req.body.cardNumber;
    const cardPin = req.body.cardPin;
    if(cardNumber && cardPin){
        data = await User.login(cardNumber, cardPin)
        if(data.status == 200){
            res.status(200).send(
                {
                   'token': data.token
                }
             );
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Logout user
router.get('/logout', async function(req, res){
    console.log('GET /login');
    // No params required, get necessary info from jwt
    const cardNumber = req.user.cardNumber;
    if(cardNumber){
        data = await User.logout(cardNumber);
        if(data.status == 200){
            res.sendStatus(200);
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

module.exports = router;