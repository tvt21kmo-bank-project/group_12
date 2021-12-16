// Import express and init router
const express = require('express');
const router = express.Router();
const auth = require('../middleware/auth.js');
const Event = require('../models/event.js')

// Show balance
router.get('/balance', auth, async function(req, res){
    console.log('GET, /balance');
    // No params required, get necessary info from jwt
    const cardId = req.user.cardId;
    if(cardId){
        data = await Event.getBalance(cardId, false);
        if(data.status == 200){
            console.log(data.balance);
            res.status(200).send(
                {
                    'balance': data.balance,
                    'creditLimit': data.credit_limit
                }
            )
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(403);
    }
});
// Show balance extended (include transactions)
router.get('/exbalance', auth, async function(req, res){
    console.log('GET, /balance');
    // No params required, get necessary info from jwt
    const cardId = req.user.cardId;
    if(cardId){
        data = await Event.getBalance(cardId, true);
        if(data.status == 200){
            console.log(data.balance);
            res.status(200).send(
                {
                    'balance': data.balance,
                    'creditLimit': data.credit_limit,
                    'transactions': data.transactions
                }
            )
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(403);
    }
});

// Browse transactions
router.post('/transactions', auth, async function(req, res){
    console.log('POST, /transactions');
    const cardId = req.user.cardId;
    const page = req.body.page;
    if(cardId && page){
        data = await Event.getTransactions(cardId, page);
        if(data.status == 200){
            res.status(200).send(
                {
                    'transactions': data.transactions
                }
            );
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Withdraw money
router.post('/withdraw', auth, async function(req, res){
    console.log('POST, /withdraw');
    const amount = req.body.amount;
    const cardId = req.user.cardId;
    const sessionCardType = req.user.sessionCardType;
    if(amount && cardId && sessionCardType){
        data = await Event.withdrawMoney(cardId, sessionCardType, amount);
        console.log(data);
        if(data.status == 200){
            res.status(200).send(
                {
                    'newBalance': data.newBalance
                }
            )
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Deposit money
router.post('/deposit', auth, async function(req, res){
    console.log('POST, /deposit');
    const amount = req.body.amount;
    const cardId = req.user.cardId;
    if(amount && cardId){
        data = await Event.depositMoney(cardId, amount);
        if(data.status == 200){
            res.status(200).send({
                'newBalance': data.newBalance
            });
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

module.exports = router;