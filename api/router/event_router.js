// Import express and init router
const express = require('express');
const router = express.Router();

// Show balance
router.get('/balance', async function(req, res){
    console.log('POST, /balance');
    // No params required, get necessary info from jwt
});

// Browse transactions
router.get('/transactions', async function(req, res){
    console.log('POST, /transactions');
    // No params required, get necessary info from jwt
});

// Withdraw money
router.post('/withdraw', async function(req, res){
    console.log('POST, /withdraw');
    const amount = req.body.amount;
});

// Deposit money
router.post('/deposit', async function(req, res){
    console.log('POST, /deposit');
    const amount = req.body.amount;
});

module.exports = router;