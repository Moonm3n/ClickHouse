#include "MetadataStorageFromLocalDisk.h"
#include <Disks/IDisk.h>


namespace DB
{

namespace ErrorCodes
{
    extern const int NOT_IMPLEMENTED;
}

MetadataStorageFromLocalDisk::MetadataStorageFromLocalDisk(DiskPtr disk_, ObjectStoragePtr object_storage_)
    : disk(disk_)
    , object_storage(object_storage_)
{
}

MetadataTransactionPtr MetadataStorageFromLocalDisk::createTransaction() const
{
    return std::make_shared<MetadataStorageFromLocalDiskTransaction>(*this, disk);
}

const std::string & MetadataStorageFromLocalDisk::getPath() const
{
    return disk->getPath();
}

bool MetadataStorageFromLocalDisk::exists(const std::string & path) const
{
    return disk->exists(path);
}

bool MetadataStorageFromLocalDisk::isFile(const std::string & path) const
{
    return disk->isFile(path);
}

bool MetadataStorageFromLocalDisk::isDirectory(const std::string & path) const
{
    return disk->isDirectory(path);
}

Poco::Timestamp MetadataStorageFromLocalDisk::getLastModified(const std::string & path) const
{
    return disk->getLastModified(path);
}

time_t MetadataStorageFromLocalDisk::getLastChanged(const std::string & path) const
{
    return disk->getLastChanged(path);
}

uint64_t MetadataStorageFromLocalDisk::getFileSize(const String & path) const
{
    return disk->getFileSize(path);
}

std::vector<std::string> MetadataStorageFromLocalDisk::listDirectory(const std::string & path) const
{
    std::vector<std::string> result;
    auto it = disk->iterateDirectory(path);
    while (it->isValid())
    {
        result.push_back(it->path());
        it->next();
    }
    return result;
}

DirectoryIteratorPtr MetadataStorageFromLocalDisk::iterateDirectory(const std::string & path) const
{
    return disk->iterateDirectory(path);
}

std::string MetadataStorageFromLocalDisk::readFileToString(const std::string &) const
{
    throw Exception(ErrorCodes::NOT_IMPLEMENTED, "readFileToString is not implemented for MetadataStorageFromLocalDisk");
}

std::unordered_map<String, String> MetadataStorageFromLocalDisk::getSerializedMetadata(const std::vector<String> &) const
{
    throw Exception(ErrorCodes::NOT_IMPLEMENTED, "getSerializedMetadata is not implemented for MetadataStorageFromLocalDisk");
}

BlobsPathToSize MetadataStorageFromLocalDisk::getBlobs(const std::string & path) const
{
    return {BlobPathWithSize(path, getFileSize(path))};
}

std::vector<std::string> MetadataStorageFromLocalDisk::getRemotePaths(const std::string & path) const
{
    return {fs::path(getPath()) / path};
}

uint32_t MetadataStorageFromLocalDisk::getHardlinkCount(const std::string & path) const
{
    /// FIXME: -1?
    return disk->getRefCount(path);
}

void MetadataStorageFromLocalDiskTransaction::writeStringToFile(const std::string & path, const std::string & data) /// NOLINT
{
    auto wb = disk->writeFile(path);
    wb->write(data.data(), data.size());
    wb->finalize();
}

void MetadataStorageFromLocalDiskTransaction::setLastModified(const std::string & path, const Poco::Timestamp & timestamp)
{
    disk->setLastModified(path, timestamp);
}

void MetadataStorageFromLocalDiskTransaction::unlinkFile(const std::string & path)
{
    disk->removeFile(path);
}

void MetadataStorageFromLocalDiskTransaction::removeRecursive(const std::string & path)
{
    disk->removeRecursive(path);
}

void MetadataStorageFromLocalDiskTransaction::createDirectory(const std::string & path)
{
    disk->createDirectory(path);
}

void MetadataStorageFromLocalDiskTransaction::createDicrectoryRecursive(const std::string & path)
{
    disk->createDirectories(path);
}

void MetadataStorageFromLocalDiskTransaction::removeDirectory(const std::string & path)
{
    disk->removeDirectory(path);
}

void MetadataStorageFromLocalDiskTransaction::moveFile(const std::string & path_from, const std::string & path_to)
{
    disk->moveFile(path_from, path_to);
}

void MetadataStorageFromLocalDiskTransaction::moveDirectory(const std::string & path_from, const std::string & path_to)
{
    disk->moveDirectory(path_from, path_to);
}

void MetadataStorageFromLocalDiskTransaction::replaceFile(const std::string & path_from, const std::string & path_to)
{
    disk->replaceFile(path_from, path_to);
}

void MetadataStorageFromLocalDiskTransaction::setReadOnly(const std::string & path)
{
    disk->setReadOnly(path);
}

void MetadataStorageFromLocalDiskTransaction::createHardLink(const std::string & /* path_from */, const std::string & /* path_from */)
{
}

void MetadataStorageFromLocalDiskTransaction::createEmptyMetadataFile(const std::string & /* path */)
{
}

void MetadataStorageFromLocalDiskTransaction::createMetadataFile(
    const std::string & /* path */, const std::string & /* blob_name */, uint64_t /* size_in_bytes */)
{
}

void MetadataStorageFromLocalDiskTransaction::addBlobToMetadata(
    const std::string & /* path */, const std::string & /* blob_name */, uint64_t /* size_in_bytes */)
{
}

void MetadataStorageFromLocalDiskTransaction::unlinkMetadata(const std::string & path)
{
    disk->removeFile(path);
    /// addOperation(std::make_unique<UnlinkFileOperation>(path, *metadata_storage_for_local.getDisk()));
}

}
