import crypto from 'crypto';

const [,, mode, input, key, iv] = process.argv;
const MAX_LENGTH = 256;

if (!['-e', '-d'].includes(mode) || !input || !key || !iv
    || key.length !== 32 || iv.length !== 16) {
    console.error('Usage: node aes256cbc.mjs <-e|-d> <input> <32 character key> <16 character iv>');
    process.exit(1);
}

if (input.length > MAX_LENGTH) {
    console.error(`Input string exceeds maximum length of ${MAX_LENGTH} characters.`);
    process.exit(1);
}

let keyBuffer, ivBuffer;
try {
    keyBuffer = Buffer.from(key, 'utf-8');
    ivBuffer = Buffer.from(iv, 'utf-8');
} catch (error) {
    console.error('Error creating buffers for key or IV:', error.message);
    process.exit(1);
}

if (mode === '-e') {
    const cipher = crypto.createCipheriv('aes-256-cbc', keyBuffer, ivBuffer);
    let encrypted = cipher.update(input, 'utf-8', 'hex');
    encrypted += cipher.final('hex');
    console.log(encrypted);
} else if (mode === '-d') {
    const decipher = crypto.createDecipheriv('aes-256-cbc', keyBuffer, ivBuffer);
    let decrypted = decipher.update(input, 'hex', 'utf-8');
    decrypted += decipher.final('utf-8');
    console.log(decrypted);
}