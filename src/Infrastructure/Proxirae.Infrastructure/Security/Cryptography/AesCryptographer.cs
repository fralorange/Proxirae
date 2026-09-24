using Proxirae.Application.Security.Cryptography;
using System.Security.Cryptography;
using System.Text;

namespace Proxirae.Infrastructure.Security.Cryptography
{
    public class AesCryptographer : ICryptographer
    {
        private const int SaltSize = 16;
        private const int KeySize = 32;
        private const int NonceSize = 12;
        private const int TagSize = 16;
        private const int Pbkdf2Iterations = 100_000;

        public string Encrypt(string plainText, string masterPassword)
        {
            if (string.IsNullOrEmpty(plainText))
            {
                return string.Empty;
            }

            if (string.IsNullOrWhiteSpace(masterPassword))
            {
                throw new ArgumentException("Master password can't be null", nameof(masterPassword));
            }

            var plainBytes = Encoding.UTF8.GetBytes(plainText);

            var salt = RandomNumberGenerator.GetBytes(SaltSize);
            var nonce = RandomNumberGenerator.GetBytes(NonceSize);

            using var kdf = new Rfc2898DeriveBytes(masterPassword, salt, Pbkdf2Iterations, HashAlgorithmName.SHA256);
            var key = kdf.GetBytes(KeySize);

            var cipherTextBytes = new byte[plainBytes.Length];
            var tag = new byte[TagSize];

            using var aesGcm = new AesGcm(key, TagSize);
            aesGcm.Encrypt(nonce, plainBytes, cipherTextBytes, tag);

            var payloadBytes = new byte[SaltSize + NonceSize + TagSize + cipherTextBytes.Length];

            Buffer.BlockCopy(salt, 0, payloadBytes, 0, SaltSize);
            Buffer.BlockCopy(nonce, 0, payloadBytes, SaltSize, NonceSize);
            Buffer.BlockCopy(tag, 0, payloadBytes, SaltSize + NonceSize, TagSize);
            Buffer.BlockCopy(cipherTextBytes, 0, payloadBytes, SaltSize + NonceSize + TagSize, cipherTextBytes.Length);

            return Convert.ToBase64String(payloadBytes);
        }

        private static string Decrypt(string cipherText, string masterPassword)
        {
            if (string.IsNullOrEmpty(cipherText))
            {
                return string.Empty;
            }

            if (string.IsNullOrWhiteSpace(masterPassword))
            {
                throw new ArgumentException("Master Password can't be null or whitespace.", nameof(masterPassword));
            }

            var payloadBytes = Convert.FromBase64String(cipherText);

            var minHeaderSize = SaltSize + NonceSize + TagSize;
            if (payloadBytes.Length < minHeaderSize)
            {
                throw new CryptographicException("The encrypted data is in the wrong format or is corrupted.", nameof(cipherText));
            }

            var salt = new byte[SaltSize];
            var nonce = new byte[NonceSize];
            var tag = new byte[TagSize];
            var cipherTextBytes = new byte[payloadBytes.Length - minHeaderSize];

            Buffer.BlockCopy(payloadBytes, 0, salt, 0, SaltSize);
            Buffer.BlockCopy(payloadBytes, SaltSize, nonce, 0, NonceSize);
            Buffer.BlockCopy(payloadBytes, SaltSize + NonceSize, tag, 0, TagSize);
            Buffer.BlockCopy(payloadBytes, minHeaderSize, cipherTextBytes, 0, cipherTextBytes.Length);

            using var kdf = new Rfc2898DeriveBytes(masterPassword, salt, Pbkdf2Iterations, HashAlgorithmName.SHA256);
            byte[] key = kdf.GetBytes(KeySize);

            var plainBytes = new byte[cipherTextBytes.Length];

            using var aesGcm = new AesGcm(key, TagSize);

            aesGcm.Decrypt(nonce, cipherTextBytes, tag, plainBytes);

            return Encoding.UTF8.GetString(plainBytes);
        }

        public bool TryDecrypt(string cipherText, string masterPassword, out string plainText)
        {
            plainText = string.Empty;

            try
            {
                plainText = Decrypt(cipherText, masterPassword);
            }
            catch
            {
                return false;
            }

            return true;
        }

        public bool IsEncrypted(string cipherText)
        {
            if (string.IsNullOrWhiteSpace(cipherText)) return false;

            try
            {
                var payloadBytes = Convert.FromBase64String(cipherText);
                return payloadBytes.Length >= (SaltSize + NonceSize + TagSize);
            }
            catch
            {
                return false;
            }
        }
    }
}
