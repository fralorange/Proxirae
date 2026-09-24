namespace Proxirae.Application.Security.Protection
{
    public interface IProtector
    {
        string Protect(string plainText);
        bool TryUnprotect(string cipherText, out string plainText);
        bool IsProtected(string cipherText);
    }
}
