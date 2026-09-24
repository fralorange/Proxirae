using Proxirae.Application.Security.Protection;
using System.Security.Cryptography;
using System.Text;

namespace Proxirae.Infrastructure.Security.Protection.Windows
{
    public class WindowsProtector : IProtector
    {
        public string Protect(string plainText)
        {
            if (string.IsNullOrEmpty(plainText))
            {
                return string.Empty;
            }

            var plainBytes = Encoding.UTF8.GetBytes(plainText);

#pragma warning disable CA1416 
            var encryptedBytes = ProtectedData.Protect(
                plainBytes,
                optionalEntropy: null,
                scope: DataProtectionScope.CurrentUser);
#pragma warning restore CA1416 

            return Convert.ToBase64String(encryptedBytes);
        }

        private static string Unprotect(string cipherText)
        {
            if (string.IsNullOrEmpty(cipherText))
            {
                return string.Empty;
            }

            var encryptedBytes = Convert.FromBase64String(cipherText);

#pragma warning disable CA1416
            var plainBytes = ProtectedData.Unprotect(
                encryptedBytes,
                optionalEntropy: null,
                scope: DataProtectionScope.CurrentUser);
#pragma warning restore CA1416

            return Encoding.UTF8.GetString(plainBytes);
        }

        public bool TryUnprotect(string cipherText, out string plainText)
        {
            plainText = string.Empty;

            try
            {
                plainText = Unprotect(cipherText);
            }
            catch
            {
                return false;
            }

            return true;
        }

        public bool IsProtected(string cipherText)
        {
            if (string.IsNullOrWhiteSpace(cipherText))
            {
                return false;
            }

            return TryUnprotect(cipherText, out _);
        }
    }
}
