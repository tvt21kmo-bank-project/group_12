const express = require('express');
const router = express.Router();
const Account = require('../models/account.js')

// Get all accounts
router.get('/', async function(req, res){
    data = await Account.getAll();
    if(data.status == 200){
        res.status(200).send(
            {
                'accounts': data.accounts
            }
        )
    } else {
        res.sendStatus(data.status);
    }
});

// Create account
router.post('/', async function(req, res){
    console.log('POST, /account');
    const ownerId = req.body.ownerId;
    if(ownerId){
        data = await Account.create(ownerId);
        if(data.status == 200){
            res.status(200).send({
                'accountId': data.accountId
            });
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Modify account
router.put('/', async function(req, res){
    console.log('PUT, /account');
    const accountId = req.body.accountId;
    const ownerId = req.body.ownerId;
    const balance = req.body.balance;
    if(accountId && (ownerId || balance)){
        data = await Account.modify(accountId, ownerId, balance);
        if(data.status == 200){
            res.sendStatus(200);
        } else {
            res.sendStatus(data.status);
        }
    } else {
        res.sendStatus(400);
    }
});

// Delete account
router.delete('/', async function(req, res){
    console.log('DELETE, /account');
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