const pool = require("../config/db.js");

module.exports = {
    async getBalance(cardId, includeTransactions){
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
        var conn = null;
        try {
            conn = await pool.getConnection();
            const getBalance = await conn.query(
                'SELECT \
                    a2.`balance`, \
                    a1.`credit_limit` \
                FROM `card` a1 \
                    INNER JOIN `account` a2 ON a1.`account_id`=a2.`id` \
                WHERE a1.`id`=?',
                cardId
            )
            if((getBalance[0] != null)){
                data.status = 200;
                data.balance = getBalance[0].balance;
                data.creditLimit = getBalance[0].credit_limit;

                if(includeTransactions == true){
                    // Get last 10 entries from transaction logs
                    transactionsQuery = await conn.query(
                        'SELECT \
                            `amount`, \
                            `timestamp`, \
                            `type` \
                        FROM `transaction_log`\
                        WHERE `card_id`=? \
                        ORDER BY `timestamp` DESC \
                        LIMIT 10',
                        cardId
                    );
                    data.transactions = transactionsQuery;
                }
            } else {
                data.status = 403;
            }
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },

    async getTransactions(cardId, page){
        console.log("man");
        /**
         * Status codes:
         * 200, Request succesful
         * 400, Bad request
         * 500, Server error
         */
        data = {
            'status': 400,
            'transactions': []
        }
        var conn = null;
        try {
            limit = 10;
            offset = 0;
            
            // For every page increment, move offset by 10
            if(page > 1){
                offset = page * 10 - 10;
            }

            conn = await pool.getConnection();
            const transactionsQuery = await conn.query(
                'SELECT \
                    `amount`, \
                    `timestamp`, \
                    `type` \
                FROM `transaction_log`\
                WHERE `card_id`=? \
                ORDER BY `timestamp` \
                LIMIT ? \
                OFFSET ?',
                [cardId, limit, offset]
            );
            data.transactions = transactionsQuery;         
            data.status = 200;
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        if(conn != null){
            conn.release();
        }
        return data
    },

    async depositMoney(cardId, amount){
        data = {
            'status': 400
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const depositQuery = await conn.query(
                'UPDATE \
                    `account` a1 \
                INNER JOIN \
                    `card` a2 ON a1.`id`=a2.`account_id`\
                SET a1.`balance`=(a1.`balance` + ?) \
                WHERE a2.`id`=?',
                [
                    amount,
                    cardId
                ]
            );
            if(depositQuery.affectedRows == 1){
                balanceQuery = await conn.query(
                    'SELECT a1.`id`, a1.`balance` FROM `account` a1 INNER JOIN `card` a2 ON a1.`id`=a2.`account_id` WHERE a2.`id`=?',
                    cardId
                );
                if(balanceQuery[0] != null){
                    data.newBalance = balanceQuery[0].balance;
                    transQuery = await conn.query(
                        'INSERT INTO `transaction_log`(`amount`,`type`,`account_id`,`card_id`) VALUES (?,1,?,?)',
                        [amount, balanceQuery[0].id, cardId]
                    );
                }
                data.status = 200;
            }
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        if(conn != null){
            conn.release();
        }
        return data
    },

    async withdrawMoney(cardId, sessionCardType, amount){
        /**
         * Status codes:
         * 200, Withdraw succesful
         * 400, Bad request
         * 460, Not enough balance to withdraw current amount
         * 461, Not enough credit
         * 500, Server error
         */
        data = {
            'status': 400,
            'newBalance': null,
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const balanceQuery = await conn.query(
                'SELECT \
                    a1.`id`, \
                    a1.`balance`, \
                    a2.`credit_limit` \
                FROM `account` a1 \
                INNER JOIN `card` a2 \
                    ON a1.`id`=a2.`account_id` \
                WHERE a2.`id`=?',
                cardId
            );
            if(balanceQuery[0] != null){
                if(sessionCardType == 1){
                    // Debit
                    if(amount < balanceQuery[0].balance){
                        updateQuery = await conn.query(
                            'UPDATE `account` SET `balance`=(`balance`-?) WHERE `id`=?',
                            [amount, balanceQuery[0].id]
                        );
                        if(updateQuery.affectedRows >= 1){
                            data.status = 200;
                            data.newBalance = (balanceQuery[0].balance - amount);
                            // Add entry to transaction log
                            transQuery = await conn.query(
                                'INSERT INTO `transaction_log`(`amount`,`type`,`account_id`,`card_id`) VALUES (?,2,?,?)',
                                [amount, balanceQuery[0].id, cardId]
                            );
                        }
                    } else {
                        data.status = 460;
                    }
                } else if (sessionCardType == 2){
                    // Credit
                    if(amount < (balanceQuery[0].balance + balanceQuery[0].credit_limit)){
                        updateQuery = await conn.query(
                            'UPDATE `account` SET `balance`=(`balance`-?) WHERE `id`=?',
                            [amount, balanceQuery[0].id]
                        );
                        if(updateQuery.affectedRows >= 1){
                            data.status = 200;
                            data.newBalance = (balanceQuery[0].balance - amount);
                            // Add entry to transaction log
                            transQuery = await conn.query(
                                'INSERT INTO `transaction_log`(`amount`,`type`,`account_id`,`card_id`) VALUES (?,2,?,?)',
                                [amount, balanceQuery[0].id, cardId]
                            );
                        }
                    } else {
                        data.status = 461;
                    }
                }
            }
        } catch (err) {
            data.status = 500;
            console.log(err);
        }
        if(conn != null){
            conn.release();
        }
        return data
    }
}; 