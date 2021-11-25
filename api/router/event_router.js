// Import express and init router
const express = require('express');
const router = express.Router();
const auth = require('../middleware/auth.js');
const Event = require('../models/event.js')

// Show balance
router.get('/balance', auth, async function(req, res){
    console.log('GET, /balance');
    // No params required, get necessary info from jwt
    const cardNumber = req.user.cardNumber;
    data = await Event.getBalance(cardNumber);
    if(data.status == 200){
        res.status(200).send(
            {
                'balance': data.balance
            }
        )
    } else {
        res.sendStatus(data.status);
    }
});

// Browse transactions
router.post('/transactions', auth, async function(req, res){
    console.log('POST, /transactions');
    const cardNumber = req.user.cardNumber;
    const page = req.body.page;
    if(cardNumber && page){
        data = await Event.getTransactions(cardNumber, page);
        if(data.status == 200){
            res.status(200).send(data.log);
        }
    }
});

// Withdraw money
router.post('/withdraw', auth, async function(req, res){
    console.log('POST, /withdraw');
    const amount = req.body.amount;
    const cardNumber = req.user.cardNumber;
    if(amount){
        data = await Event.withdrawMoney(cardNumber, amount);
        if(data.status == 200){
            res.status(200).send(
                {
                    'newBalance': data.newBalance
                }
            )
        } else {
            res.sendStatus(data.status);
        }
    }
});

// Deposit money
router.post('/deposit', auth, async function(req, res){
    console.log('POST, /deposit');
    const amount = req.body.amount;
    const cardNumber = req.user.cardNumber;
    if(amount && cardNumber){
        data = await Event.depositMoney(cardNumber, amount);
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