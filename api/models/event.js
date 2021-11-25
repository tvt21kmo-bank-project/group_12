const pool = require("../config/db.js");

module.exports = {
    async getBalance(cardNumber){
        /**
         * Status codes:
         * 200, Request succesful
         * 400, Bad request
         * 403, Cant find account with current card number
         * 500, Server error
         */
        data = {
            'status': 400,
            'balance': null
        }
        try {
            const conn = await pool.getConnection();
            const getBalance = await conn.query(
                'SELECT `account`.`balance` FROM `card` INNER JOIN `account` ON `card`.`account_id`=`account`.`id` WHERE `card`.`card_number`=?',
                cardNumber
            )
            if((getBalance[0] != null)){
                data.status = 200;
                data.balance = getBalance[0].balance;
            } else {
                data.status = 403;
            }
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        return data;
    },

    async getTransactions(cardNumber, page){
        /**
         * Status codes:
         * 200, Request succesful
         * 400, Bad request
         * 500, Server error
         */
        data = {
            'status': 400,
            'log': []
        }
        try {
            limit = 10;
            offset = 0;
            
            // For every page increment, move offset by 10
            if(page > 1){
                offset = page * 10 - 10;
            }

            const conn = await pool.getConnection();
            const transactionsQuery = await conn.query(
                'SELECT `transaction_log`.`amount`, `transaction_log`.`timestamp`, `transaction_log`.`type` FROM `transaction_log` LEFT JOIN `card` ON `transaction_log`.`card_id`=`card`.`id` WHERE `card`.`card_number`=? ORDER BY `transaction_log`.`timestamp` LIMIT ? OFFSET ?',
                [cardNumber, limit, offset]
            )
            data.log = transactionsQuery;         
            data.status = 200;
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        return data
    },

    async depositMoney(cardNumber, amount){
        data = {
            'status': 400
        }
        console.log(amount);
        try {
            const conn = await pool.getConnection();
            const depositQuery = await conn.query(
                'UPDATE `account` INNER JOIN `card` ON `account`.`id`= `card`.`account_id` SET `balance`=(`balance`+?) WHERE `card`.`card_number`=?',
                [
                    amount,
                    parseInt(cardNumber)
                ]
            );
            if(depositQuery.affectedRows == 1){
                data.status = 200;
            }
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        return data
    },

    async withdrawMoney(cardNumber, amount){
        /**
         * Status codes:
         * 200, Withdraw succesful
         * 400, Bad request
         * 403, Not enough balance to withdraw current amount
         * 500, Server error
         */
        data = {
            'status': 400,
            'newBalance': null,
        }
        try {
            const conn = await pool.getConnection();
            const checkBalanceQuery = await conn.query(
                'SELECT `account`.`id`, `account`.`balance`, `account`.`credit_limit`, `account`.`type` FROM `account` INNER JOIN `card` ON `account`.`id`=`card`.`account_id` WHERE `card`.`card_number`=?',
                cardNumber
            );
            if(checkBalanceQuery[0] != null){
                if(checkBalanceQuery[0].type == 1){
                    // Account type credit
                    if(amount > (checkBalanceQuery[0].balance + checkBalanceQuery[0].credit_limit)){
                        data.status = 403;
                        return data
                    }
                } else if(checkBalanceQuery[0].type == 2){
                    // Account type debit
                    if(amount > checkBalanceQuery[0].balance){
                        data.status = 403;
                        return data
                    }
                }
                const withdrawQuery = await conn.query(
                    'UPDATE `account` SET `balance`=(`balance`-?) WHERE `id`=?',
                    [
                        amount,
                        checkBalanceQuery[0].id
                    ]
                )
                if(withdrawQuery.affectedRows == 1){
                    // Calculate new balance from existing data to save 1 db query
                    data.newBalance = checkBalanceQuery[0].balance - amount;
                    data.status = 200;
                }
            }
        } catch (err) {
            data.status = 500;
            console.log(err);
        }
        return data
    }
}; 