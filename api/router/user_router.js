const express = require('express');
const router = express.Router();
const User = require('../models/user.js')
const auth = require('../middleware/auth.js');

// Create user
router.post('/', async function(req, res){
    console.log('POST, /user');
    const name = req.body.name;
    const address = req.body.address;
    const phone = req.body.phone;
    if(name && address && phone){
        data = await User.create(name, address, phone);
        if(data.status == 200){
            res.status(200).send({
                'userId': data.userId
            });
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Modify user
router.put('/', async function(req, res){
    const userId = req.body.userId;
    const name = req.body.name;
    const address = req.body.address;
    const phone = req.body.phone;
    if(userId && (name || address || phone)){
        data = await User.modify(userId, name, address, phone);
        if(data.status == 200){
            res.sendStatus(200);
        } else {
            res.sendStatus(data.status);
        }
    }
});

// Delete user
router.delete('/', async function(req, res){
    const userId = req.body.userId;
    if(userId){
        data = await User.delete(userId);
        if(data.status == 200){
            res.sendStatus(200);
        } else {
            res.sendStatus(data.status);
        }
    }
});

// Login user
router.post('/login', async function(req, res){
    console.log('POST, /login');
    console.log(req.body);
    const cardId = req.body.cardId;
    const cardPin = req.body.cardPin;
    if(cardId && cardPin){
        data = await User.login(cardId, cardPin)
        console.log(data);
        if(data.status == 200){
            res.status(200).send(
                {
                   'token': data.token,
                   'cardType': data.cardType,
                   'creditLimit': data.creditLimit,
                   'userName': data.userName,
                   'userAddress': data.userAddress,
                   'userPhone': data.userPhone,
                   'ownerName': data.ownerName,
                   'ownerAddress': data.ownerAddress,
                   'ownerPhone': data.ownerPhaone
                }
             );
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});


// Update session
router.put('/login', auth, async function(req, res){
    console.log('PUT, /login');
    console.log(req.user);
    const cardId = req.user.cardId;
    const cardType = req.user.cardType;
    const sessionCardType = req.body.sessionCardType;
    if(cardId && cardType){
        data = await User.updateSessionCardType(cardId, cardType, sessionCardType);
        if(data.status == 200){
            res.status(200).send(
                {
                   'updatedToken': data.updatedToken
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