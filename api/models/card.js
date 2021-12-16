const pool = require("../config/db.js");
const argon2 = require("argon2");

module.exports = {
    async create(cardPin, cardType, creditLimit, userId, accountId){
        /**
         * Requires account and user to be created first
         */
        data = {
            'status': 400,
            'cardId': null
        }
        var conn = null;
        try {
            // Hash pincode with argon2 before inserting it to database
            const hashedPin = await argon2.hash(cardPin);

            conn = await pool.getConnection();
            const createQuery = await conn.query(
                'INSERT INTO `card`( \
                    `account_id`, \
                    `card_pin`, \
                    `card_type`, \
                    `credit_limit`, \
                    `user_id` \
                ) VALUES (?,?,?,?,?)',
                [accountId, hashedPin, cardType, creditLimit, userId]
            );
            if(createQuery.affectedRows >= 1 && createQuery.insertId){
                data.cardId = createQuery.insertId;
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

    async delete(cardId){
        data = {
            'status': 400
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const deleteQuery = await conn.query(
                'DELETE FROM `card` WHERE `id`=?',
                cardId
            );
            if(deleteQuery.affectedRows >= 1){
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
}; 