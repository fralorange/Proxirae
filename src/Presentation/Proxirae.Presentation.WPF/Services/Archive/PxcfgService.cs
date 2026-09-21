using Proxirae.Application.Services.Archive;
using System.IO;
using System.IO.Compression;

namespace Proxirae.Presentation.WPF.Services.Archive;

public class PxcfgService : IArchiveService
{
    public bool CreateArchive(string outputPath, params string[] filePaths)
    {
        if (filePaths.Length is < 1 or > 2 ||
            filePaths.Any(path => !Path.Exists(path)))
        {
            return false;
        }

        if (File.Exists(outputPath))
        {
            File.Delete(outputPath);
        }

        using var archive = ZipFile.Open(outputPath, ZipArchiveMode.Create);

        foreach (var path in filePaths)
        {
            archive.CreateEntryFromFile(path, Path.GetFileName(path));
        }

        return true;
    }

    public bool ExtractArchive(string inputPath, string targetDirectory)
    {
        if (!File.Exists(inputPath))
        {
            return false;
        }

        try
        {
            using var archive = ZipFile.OpenRead(inputPath);

            var jsonEntries = archive.Entries
                .Where(e => e.Name.EndsWith(".json", StringComparison.OrdinalIgnoreCase))
                .ToArray();

            if (jsonEntries.Length is < 1 or > 2)
            {
                return false;
            }

            Directory.CreateDirectory(targetDirectory);

            foreach (var entry in jsonEntries)
            {
                var destPath = Path.Combine(targetDirectory, entry.Name);
                entry.ExtractToFile(destPath, overwrite: true);
            }

            return true;
        }
        catch (InvalidDataException)
        {
            return false;
        }
    }
}