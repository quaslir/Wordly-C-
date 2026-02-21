import express from "express"
import cors from "cors"
import dotenv from "dotenv"
import mongoose from "mongoose"
import { User } from "../src/models/user.js"
dotenv.config();
const app = express();

app.use(express.json());

app.use(cors());

const url:string = process.env.MONGO_URL as string;
const connectDb = async() => {
    try {
        await mongoose.connect(url);
        console.log("MongoDB connected");
    } catch(error) {
        console.error("Failed to connect MongoDB");
        process.exit(1);
    }
}
connectDb();


app.get("/leaderboard", async(req, res) => {
return res.json(await User.find().select("username xp").limit(10));
});

interface updateXp {
    username : string,
    xp: string
};

app.post("/update-leaderboard", async(req, res) => {
    const {username, xp} :updateXp = req.body;
    const toNumberXp:number = Number(xp);
    console.log(username, toNumberXp);
    if(!username || !xp) return res.status(400);

    try {
        const result = await User.updateOne({"username" : username}, {
            $set : {xp: toNumberXp}
        });

        if(result.matchedCount == 0) {
            return res.status(404);
        }

        return res.status(200);
    } catch(error) {
        console.error("Failed to update user, error: ", error);
        return res.status(500);
    }
});

app.listen(3000, ()=> console.log("Server is working on 3000 port"));