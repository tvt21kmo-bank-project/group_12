const pool = require("../config/db.js");
const argon2 = require("argon2");
const jwt = require("jsonwebtoken");

module.exports = {
    async getAll(){
        data = {
            'accounts': []
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const getquery = await conn.query(
                'SELECT * FROM `account`'
            );
            data.accounts = getquery;
            data.status = 200;
        } catch (err) {
            console.log(err);
            data.status = 500;
        }
        if(conn != null){
            conn.release();
        }
        return data;
    },
    
    async create(ownerId){
        data = {
            'status': 400,
            'accountId': null
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const createQuery = await conn.query(
                'INSERT INTO `account`(`owner_user_id`) VALUES (?)',
                ownerId
            );
            if(createQuery.affectedRows >= 1){
                data.accountId = createQuery.insertId;
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

    async modify(accountId, ownerId, balance){
        data = {
            'status': 400
        }
        // Build query string
        updateTerms = [];
        if(ownerId != null){
            updateTerms.push('`owner_user_id`=' + ownerId);
        }
        if(balance != null){
            updateTerms.push('`balance`=' + balance);
        }
        updateString = 'UPDATE `account` SET ' + updateTerms.join(', ') + ' WHERE `id`=' + accountId;
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

    async delete(accountId){
        data = {
            'status': 400
        }
        var conn = null;
        try {
            conn = await pool.getConnection();
            const deleteQuery = await conn.query(
                'DELETE FROM `account` WHERE `id`=?',
                accountId
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