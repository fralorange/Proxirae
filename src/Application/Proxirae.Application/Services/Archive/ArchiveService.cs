using Proxirae.Application.Services.Archive;
using System.IO;
using System.IO.Compression;

namespace Proxirae.Presentation.WPF.Services.Archive;

public class ArchiveService : IArchiveService
{
    public bool CreateArchive(string outputPath, IReadOnlyDictionary<string, byte[]> entries)
    {
        if (entries == null || entries.Count == 0)
        {
            return false;
        }

        try
        {
            if (File.Exists(outputPath))
            {
                File.Delete(outputPath);
            }

            using var fs = new FileStream(outputPath, FileMode.Create, FileAccess.Write);
            using var archive = new ZipArchive(fs, ZipArchiveMode.Create);

            foreach (var kvp in entries)
            {
                string fileName = kvp.Key;
                byte[] fileBytes = kvp.Value;

                var entry = archive.CreateEntry(fileName);

                using var entryStream = entry.Open();
                entryStream.Write(fileBytes, 0, fileBytes.Length);
            }

            return true;
        }
        catch
        {
            return false;
        }
    }

    public IReadOnlyDictionary<string, byte[]>? ExtractArchive(string inputPath)
    {
        if (!File.Exists(inputPath))
        {
            return null;
        }

        try
        {
            var result = new Dictionary<string, byte[]>(StringComparer.OrdinalIgnoreCase);

            using var archive = ZipFile.OpenRead(inputPath);

            foreach (var entry in archive.Entries)
            {
                if (string.IsNullOrEmpty(entry.Name)) continue;

                using var entryStream = entry.Open();

                using var ms = new MemoryStream();
                entryStream.CopyTo(ms);

                result[entry.Name] = ms.ToArray();
            }

            return result;
        }
        catch (InvalidDataException)
        {
            return null;
        }
        catch
        {
            return null;
        }
    }
}