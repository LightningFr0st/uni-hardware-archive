----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/22/2025 10:26:14 PM
-- Design Name: 
-- Module Name: comb_dev_10_tb - Behavioral
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
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity comb_dev_10_tb is
--  Port ( );
end comb_dev_10_tb;

architecture Behavioral of comb_dev_10_tb is

component comb_dev_10_beh
    Port ( X : in STD_LOGIC;
           Y : in STD_LOGIC;
           Z : in STD_LOGIC;
           F : out STD_LOGIC);
end component;

component comb_dev_10_struct
    Port ( X : in STD_LOGIC;
           Y : in STD_LOGIC;
           Z : in STD_LOGIC;
           F : out STD_LOGIC);
end component;

signal test_vector : std_logic_vector (2 downto 0);

signal x_int : std_logic;
signal y_int : std_logic;
signal z_int : std_logic;


signal f_beh : std_logic;
signal f_struct : std_logic;

signal error : std_logic;

constant period : time := 25 ns;

begin

    comb_beh: comb_dev_10_beh port map(
        x => x_int,
        y => y_int,
        z => z_int,
        f => f_beh
     );
     
     comb_struct: comb_dev_10_struct port map(
        x => x_int,
         y => y_int,
         z => z_int,
         f => f_struct
     );
     
     x_int <= test_vector(2);
     y_int <= test_vector(1);
     z_int <= test_vector(0);

     error <= f_struct xor f_beh;

    tb_proc : process
        variable error_count : integer := 0;
     begin
        
        report "Starting testbench...";
        
        for i in 0 to 7 loop
            test_vector <= std_logic_vector(to_unsigned(i, test_vector'length));
            wait for period;
            
            if error = '1' then
                error_count := error_count + 1;
                report "ERROR at test vector: " & integer'image(i);
            end if;
        end loop;
        
        if error_count = 0 then
            report "ALL TESTS PASSED! Both implementations are equivalent." severity note;
        else
            report "TEST FAILED: "& integer'image(error_count) severity note;
        end if;
        
        report "End of simulation" severity failure;
     
     end process;
end Behavioral;
