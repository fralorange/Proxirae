namespace Proxirae.Application.Security.Cryptography
{
    public interface ICryptographer
    {
        string Encrypt(string plainText, string masterPassword);
        bool TryDecrypt(string cipherText, string masterPassword, out string plainText);
        bool IsEncrypted(string cipherText);
    }
}
