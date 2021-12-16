const express = require('express');
const router = express.Router();
const Card = require('../models/card.js')

// Get all cards
router.get('/', async function(req, res){

});

// Create card
router.post('/', async function(req, res){
    const cardPin = req.body.cardPin;
    const cardType = req.body.cardType;
    const creditLimit = req.body.userId;
    const userId = req.body.userId;
    const accountId = req.body.accountId;
    if(cardPin && cardType && creditLimit && userId && accountId){
        data = await Card.create(cardPin, cardType, creditLimit, userId, accountId);
        if(data.status == 200){
            res.status(200).send({
                'cardId': data.cardId
            });
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Modify card
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

// Delete card
router.delete('/', async function(req, res){
    const accountId = req.body.accountId;
    if(accountId){
        data = await Account.delete(accountId);
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