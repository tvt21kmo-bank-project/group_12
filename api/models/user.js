const pool = require("../config/db.js");
const argon2 = require("argon2");
const jwt = require("jsonwebtoken");

module.exports = {
    async login(cardNumber, cardPin){
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
        try {
            const conn = await pool.getConnection();
            const loginQuery = await conn.query(
                'SELECT a1.`id`, a1.`card_number`, a1.`card_pin`, a2.`name` FROM `card` a1 INNER JOIN `user` a2 ON a1.`user_id`=a2.`id` WHERE a1.`card_number`=?',
                cardNumber
            )
            // If card number exists in database and pin matches
            if((loginQuery[0] != null) && await argon2.verify(loginQuery[0].card_pin, cardPin.toString())){
                const token = jwt.sign(
                    {
                        cardNumber: loginQuery[0].card_number
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
                    data.username = loginQuery[0].name;
                    data.status = 200;
                }
            } else {
                data.status = 403;
            }
            conn.release();
        } catch (err){
            data.status = 500;
            console.log(err);
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