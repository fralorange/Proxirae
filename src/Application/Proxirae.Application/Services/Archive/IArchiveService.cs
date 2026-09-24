namespace Proxirae.Application.Services.Archive
{
    public interface IArchiveService
    {
       
        bool CreateArchive(string outputPath, IReadOnlyDictionary<string, byte[]> entries);
        IReadOnlyDictionary<string, byte[]>? ExtractArchive(string inputPath);
    }
}
