const crypto = require('crypto')

// Generate an ECDH object for geekA
const geek = crypto.createECDH('prime256v1')

//bob的私鑰
geek.setPrivateKey(Buffer.from("b25ee91a6f8030a6a0652ad3710cda83b666b1232cf1f5c7d589d48551c18524", "hex"))
const geekAPublicKey = geek.getPublicKey()  //Bob的公鑰
const geekAPrivateKey = geek.getPrivateKey()    //Bob的私鑰


//Alice的公鑰


// const ecdhSecret = geek.computeSecret(geekAPublicKey)   //用自己的PUK+PRK算密鑰
// const ecdhSecret2 = geek.computeSecret(Buffer.from(geekAPublicKey.toString("hex"), "hex"))  //用自己的PUK+PRK算密鑰(用字串輸入PUK)
const ecdhSecret2 = geek.computeSecret(Buffer.from("043d8164a70370408f6d3e4a63ad1ce82163a0000e356789b2a90a8d0d88b1cd550b4796fbbc2b776a97056a4c010ebcde57d38bde2e95be59c6f333ba075d2a81", "hex"))  //用Alice的PUK+自己的PRK算密鑰(用字串輸入PUK)


console.log('geekAPublicKey:', geekAPublicKey.toString("hex"),geekAPublicKey.toString("hex").length/2)
console.log('geekAPrivateKey', geekAPrivateKey.toString("hex"),geekAPrivateKey.toString("hex").length/2)
//
//
// console.log('ecdhSecret',ecdhSecret.toString("hex"),ecdhSecret.toString("hex").length/2)
console.log('ecdhSecret2',ecdhSecret2.toString("hex"),ecdhSecret2.toString("hex").length/2)