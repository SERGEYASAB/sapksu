#include "FileLogger.h"

FileLogger::FileLogger(const QString &configFileName, const QString &configGroupName)
    : ILogger(configFileName, configGroupName)
{
    recentBufferLine = 0;
    currentBufferSize = 0;
}

FileLogger::~FileLogger()
{
    write(); //Записываем остатки из буфера
}

quint64 FileLogger::getMaxBufferSize()
{
    return maxBufferSize;
}

void FileLogger::setMaxBufferSize(quint64 size)
{
    maxBufferSize = size;
}

void FileLogger::log(LoggerTaskManager::LogLevel level, const QString &text)
{
    if(level & filter())
    {
        if(currentBufferSize + text.size() > maxBufferSize)
            write();
        buffer.append(text + "\r\n");
        currentBufferSize += buffer.last().size();
    }
}

void FileLogger::write()
{
    file.setFileName(recentFileName.isEmpty() ? LOGGER_FILE_NAME : recentFileName);
    if(file.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        QTextStream out(&file);
        quint64 bytesWritten = 0;
        for(auto line : buffer)
        {
            if(file.size() + bytesWritten + line.size() > maxFileSize)
            {
                file.close();
                file.setFileName(LOGGER_FILE_NAME);
                if(!file.open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    qDebug()<<"Не удалось создать файл лога " + file.fileName();
                    return;
                }
                bytesWritten = 0;
            }
            out<<line;
            bytesWritten += line.size();
        }
        file.close();
        recentFileName = file.fileName();
        buffer.clear();
    }
    else
        qDebug()<<"Не удалось открыть файл лога "<<file.fileName();
    currentBufferSize = 0;
}

qint64 FileLogger::getMaxFileSize() const
{
    return maxFileSize;
}

void FileLogger::setMaxFileSize(const qint64 &value)
{
    maxFileSize = value;
}

