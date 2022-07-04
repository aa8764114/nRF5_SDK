const crypto = require('crypto')

// Generate an ECDH object for geekA
const geek = crypto.createECDH('prime256v1')

const geekAPublicKey = geek.generateKeys()
const geekAPrivateKey = geek.getPrivateKey()
const ecdhSecret = geek.computeSecret(geekAPublicKey)
// const ecdhSecret2 = geek.computeSecret(Buffer.from("0425cf07effee60e79403b95a07518f27aae0f3e5bbe6e6f0211d2d9b6d47a0398341fd054a5cd00593a5cf8e6424b50ccedd7b1c0c2947c2bbd409e9ebaf67e29", "hex"))
const ecdhSecret2 = geek.computeSecret(Buffer.from(geekAPublicKey.toString("hex"), "hex"))
console.log('geekAPublicKey:', geekAPublicKey.toString("hex"),geekAPublicKey.toString("hex").length/2)
console.log('geekAPrivateKey', geekAPrivateKey.toString("hex"),geekAPrivateKey.toString("hex").length/2)
console.log('ecdhSecret',ecdhSecret.toString("hex"),ecdhSecret.toString("hex").length/2)
console.log('ecdhSecret2',ecdhSecret2.toString("hex"),ecdhSecret2.toString("hex").length/2)
// double balance[5] = {1000.0, 2.0, 3.4, 7.0, 50.0};
// console.log('geekAPublicKey',geekAPublicKey.toString("hex").length/2)