package it.giacomobergami.knobab.gui;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.dataformat.csv.CsvMapper;
import com.fasterxml.jackson.dataformat.csv.CsvSchema;

import javax.swing.*;

public class KnoBABGui {

    public static void main(String args[]) throws JsonProcessingException {
        try {
            // Set System L&F
//            UIManager.setLookAndFeel(
//                    UIManager.getSystemLookAndFeelClassName());
            UIManager.setLookAndFeel(
                    UIManager.getCrossPlatformLookAndFeelClassName());
        } catch (UnsupportedLookAndFeelException | ClassNotFoundException | InstantiationException | IllegalAccessException e) {
            // handle exception
        }
        var window = new EntryPoint("http://localhost:8795/query");


    }
}
