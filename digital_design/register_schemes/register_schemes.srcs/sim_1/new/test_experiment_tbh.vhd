----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 04:14:28 PM
-- Design Name: 
-- Module Name: test_experiment_tbh - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity test_experiment_tbh is
--  Port ( );
end test_experiment_tbh;

architecture Behavioral of test_experiment_tbh is

    signal CLK       : std_logic := '0';
    signal Reset     : std_logic := '0';
    signal Load      : std_logic := '0';
    signal Seed      : std_logic_vector(2 downto 0) := "001";
    signal Signature : std_logic_vector(0 to 2);

begin
    uut: entity work.test_experiment
        port map (
            CLK       => CLK,
            Reset     => Reset,
            Load      => Load,
            Seed      => Seed,
            Signature => Signature
        );

    clk_process : process
    begin
        while true loop
            CLK <= '0';
            wait for 5 ns;
            CLK <= '1';
            wait for 5 ns;
        end loop;
    end process;

    stim_proc : process
        variable i : integer;
    begin
        Reset <= '1';
        Load  <= '0';
        wait for 20 ns;
        Reset <= '0';

        Load <= '1';
        wait until rising_edge(CLK);
        Load <= '0';

        for i in 0 to 6 loop
            wait until rising_edge(CLK);
        end loop;

        wait for 10 ns;
        assert false
            report "Test experiment finished, Signature = " & 
                   std_logic'image(Signature(0)) &
                   std_logic'image(Signature(1)) &
                   std_logic'image(Signature(2))
            severity note;
        wait;
    end process;

end architecture;
