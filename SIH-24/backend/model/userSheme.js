const mongoose = require("mongoose")


 const userSchema =  mongoose.Schema( {
    id : Number ,
    name : { type : String , required : true} ,
    password : { type : String , required : true} ,
    
 })






  // use like  {  post : mongoose.modal("post")..... , User : mongoose.modal("")}

 module.exports = mongoose.model( "Users" , userSchema)