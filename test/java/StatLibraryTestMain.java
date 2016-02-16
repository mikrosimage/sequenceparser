import java.io.IOException;

import eu.sequence.parser.EType;
import eu.sequence.parser.Item;
import eu.sequence.parser.ItemStat;

public class StatLibraryTestMain {
    public static void main(String[] args) throws IOException {
        if (args.length == 0) {
            System.out.println("You have to specify a path");
            return;
        }
        try {
            System.loadLibrary("java_sequenceParser");
            final String path = args[0];
            final Item item = new Item(EType.eTypeFolder, path);
            final ItemStat itemStat = new ItemStat(item);
            displayFolderContent(itemStat);
        } catch (Exception e) {
            System.out.println("Error");
            e.printStackTrace();
        }
    }

    private static void displayFolderContent(ItemStat itemStat) {
        System.out.println("=====================================");
        System.out.println("Size : " + itemStat.getSize());
        System.out.println("Real Size : " + itemStat.getRealSize());
        System.out.println("Size on disk : " + itemStat.getSizeOnDisk());
        System.out.println("Full Hard links : " + itemStat.getFullNbHardLinks());
        System.out.println("Hardlinks : " + itemStat.getNbHardLinks());
        System.out.println("Device : " + itemStat.getDeviceId());
    }
}
