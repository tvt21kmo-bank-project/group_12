const jwt = require("jsonwebtoken");

const verifyToken = (req, res, next) => {
    const authHeader = req.headers['authorization'];
    if(authHeader != null){
        const bearer = authHeader.split(" ");
        try {
            const decoded = jwt.verify(bearer[1], process.env.TOKEN_SECRET);
            req.user = decoded;
        } catch (err) {
            return res.sendStatus(401);
        }
    } else {
        return res.sendStatus(401);
    }
    return next();
}

module.exports = verifyToken;