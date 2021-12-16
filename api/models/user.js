const pool = require("../config/db.js");
const argon2 = require("argon2");
const jwt = require("jsonwebtoken");

module.exports = {
    async create(name, address, phone){
        data = {
            'status': 400,
            'userId': null
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const createQuery = await conn.query(
                'INSERT INTO `user`(`name`,`address`,`phone`) VALUES (?,?,?)',
                [name, address, phone]
            );
            if(createQuery.affectedRows >= 1){
                data.userId = createQuery.insertId;
                data.status = 200;
            }
            console.log(createQuery);
        } catch (err){
            console.log(err);
            data.status = 500;
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },

    async modify(userId, name, address, phone){
        data = {
            'status': 400
        }

        // Build query string
        updateTerms = [];
        if(name != null){
            updateTerms.push('`name`="' + name + '"');
        }
        if(address != null){
            updateTerms.push('`address`="' + address + '"');
        }
        if(phone != null){
            updateTerms.push('`phone`="' + phone + '"');
        }
        updateString = 'UPDATE `user` SET ' + updateTerms.join(', ') + ' WHERE `id`=' + userId;

        // Connect and execute query
        var conn = null;
        try {
            conn = await pool.getConnection();
            const updateQuery = await conn.query(
                updateString
            );
            if(updateQuery.affectedRows >= 1){
                data.status = 200;
            }
        } catch (err){
            console.log(err);
            data.status = 500;
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },

    async delete(userId){
        data = {
            'status': 400
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const deleteQuery = await conn.query(
                'DELETE FROM `user` WHERE `id`=?',
                userId
            );
            if(deleteQuery.affectedRows >= 1){
                data.status = 200;
            }
        } catch (err){
            console.log(err);
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },

    async login(cardId, cardPin){
        /* Response codes:
         * 200, Login succesful
         * 400, Bad request
         * 403, Invalid card number or pin
         * 500, Server error
         */
        data = {
            'status': 400,
            'token': null,
            'username': null,
            'cardNumber': null
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const loginQuery = await conn.query(
                'SELECT \
                    a1.`id`, \
                    a1.`card_pin`, \
                    a1.`card_type`, \
                    a1.`credit_limit`, \
                    a1.`account_id`, \
                    a2.`name`, \
                    a2.`address`, \
                    a2.`phone`, \
                    a2.`id` as "user_id", \
                    a4.`name` as "owner_name", \
                    a4.`address` as "owner_address", \
                    a4.`phone` as "owner_phone" \
                FROM `card` a1 \
                INNER JOIN `user` a2 \
                    ON a1.`user_id`=a2.`id` \
                INNER JOIN `account` a3 \
                    ON a1.`account_id`=a3.`id` \
                INNER JOIN `user` a4 \
                    ON a3.`owner_user_id`=a4.`id` \
                WHERE a1.`id`=? \
                LIMIT 1',
                cardId
            )
            console.log(loginQuery[0]);
            // If card number exists in database and pin matches
            if((loginQuery[0] != null) && await argon2.verify(loginQuery[0].card_pin, cardPin.toString())){
                var sessionCardType = null;
                if(loginQuery[0].card_type == 1){
                    sessionCardType = 1;
                }
                // Create JWT for handling login session
                const token = jwt.sign(
                    {
                        cardId: loginQuery[0].id,
                        cardType: loginQuery[0].card_type,
                        sessionCardType: sessionCardType
                    },
                    process.env.TOKEN_SECRET,
                    {
                        expiresIn: process.env.JWT_EXPIRE_TIME,
                    }
                );
                // Update jwt to database
                const updateToken = await conn.query(
                    "UPDATE `card` SET `token`=? WHERE `id`=?",
                    [token, loginQuery[0].id]
                )
                if(updateToken.affectedRows == 1){
                    data.token = token;
                    data.cardType = loginQuery[0].card_type;
                    data.creditLimit = loginQuery[0].credit_limit;
                    data.accountId = loginQuery[0].account_id;
                    data.userName = loginQuery[0].name;
                    data.userAddress = loginQuery[0].address;
                    data.userPhone = loginQuery[0].phone;
                    data.ownerName = loginQuery[0].owner_name;
                    data.ownerAddress = loginQuery[0].owner_address;
                    data.ownerPhone = loginQuery[0].owner_phone;
                    data.status = 200;
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

    async updateSessionCardType(cardId, cardType, sessionCardType){
        data = {
            'status': 400,
            'updatedToken': null
        }
        var conn = null;
        try {
            // Create new JWT with updated fields
            const token = jwt.sign(
                {
                    cardId: cardId,
                    cardType: cardType,
                    sessionCardType: sessionCardType
                },
                process.env.TOKEN_SECRET,
                {
                    expiresIn: process.env.JWT_EXPIRE_TIME,
                }
            );
            conn = await pool.getConnection();
            const updateQuery = await conn.query(
                'UPDATE `card` SET `token`=? WHERE `id`=?',
                [token, cardId]
            );
            if(updateQuery.affectedRows >= 1){
                data.status = 200;
                data.updatedToken = token;
            }
        } catch (err) {
            data.status = 500;
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },

    async logout(cardNumber){
        /* Response codes:
         * 200, Logout succesful
         * 400, Bad request
         * 500, Server error
         */
        data = {
            'status': 400
        }
        try {
            const conn = await pool.getConnection();
            const logoutQuery = await conn.query(
                'UPDATE `card` SET `token`=NULL WHERE `card_number`=?',
                cardNumber
            );
            if(logoutQuery.affectedRows == 1){
                data.status = 200;
            }
            conn.release();
        } catch (err){
            data.status = 500;
            console.log(err);
        }
        return data
    }
}; 