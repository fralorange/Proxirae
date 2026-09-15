namespace Proxirae.Application.Services.Archive
{
    public interface IArchiveService
    {
        void CreateArchive(string outputPath, params string[] filePaths);
        bool ExtractArchive(string inputPath, string targetDirectory);
    }
}
